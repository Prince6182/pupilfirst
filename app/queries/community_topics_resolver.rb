class CommunityTopicsResolver < ApplicationQuery
  property :community_id
  property :topic_category_id
  property :target_id
  property :search

  def community_topics
    if search.present?
      applicable_topics.where('topics.title ILIKE ?', "%#{search}%")
    else
      applicable_topics
    end.includes(first_post: :creator)
  end

  private

  def authorized?
    return false if current_user.blank?

    (current_user.courses & community.courses).present? || (current_user.school_admin.present? && current_user.school_id == community.school_id)
  end

  def community
    @community ||= Community.find(community_id)
  end

  def applicable_topics
    by_category =
      if topic_category_id.present?
        community.topics.live.where(topic_category_id: topic_category_id)
      else
        community.topics.live
      end

    if target_id.present?
      by_category.where(target_id: target_id)
    else
      by_category
    end.order('created_at DESC')
  end
end